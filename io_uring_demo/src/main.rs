use io_uring::{opcode, types, IoUring};
use std::os::unix::io::AsRawFd;
use std::io;
use std::collections::HashMap;
use std::net::TcpListener;

const SQ_SIZE: u32 = 4;
const CQ_SIZE: u32 = 8;

enum EchoState {
    Read,
    Write,
}

struct EchoCtx {
    fd: u64,
    buf: Vec<u8>,
    start: usize,
    end: usize,
    state: EchoState    
}

impl EchoCtx {
    fn buf_size(&self) -> usize {
        self.end - self.start
    }
}

enum Ctx {
    Accept,
    Echo(EchoCtx)
}

fn main() -> io::Result<()> {
    let mut ring = IoUring::builder().setup_cqsize(CQ_SIZE).build(SQ_SIZE)?;
    let mut fd_ctx = HashMap::new();
    

    let listener = TcpListener::bind("127.0.0.1:6789")?;
    fd_ctx.insert(listener.as_raw_fd() as u64, Ctx::Accept);
    let accept_e = opcode::Accept::new(types::Fd(listener.as_raw_fd()), std::ptr::null_mut() as _, std::ptr::null_mut() as _)
        .build()
        .user_data(listener.as_raw_fd() as _);


    unsafe {
        ring.submission()
            .push(&accept_e)
            .expect("submission queue is full");
    }

    loop {
        ring.submit_and_wait(1)?;
        while !ring.completion().is_empty() {
            let cqe: io_uring::cqueue::Entry = ring.completion().next().expect("completion queue is empty");
            let mut fd_remove = None;
            let ctx = fd_ctx.get_mut(&cqe.user_data());
            if let Some(ctx) = ctx {
                match ctx {
                    Ctx::Accept => {
                        assert!(cqe.result() >= 0, "accept error: {}", cqe.result());
                        let fd = cqe.result();
                        println!("accepted a connection");
                        let mut echo_ctx = EchoCtx {
                            fd: fd as _,
                            buf: vec![0u8; 128],
                            start: 0,
                            end: 0,
                            state: EchoState::Read
                        };
                        let read_e = opcode::Read::new(types::Fd(fd), echo_ctx.buf.as_mut_ptr(), echo_ctx.buf.len() as _)
                            .build()
                            .user_data(fd as _);
                        fd_ctx.insert(fd as u64, Ctx::Echo(echo_ctx));
                        unsafe {
                            ring.submission()
                                .push(&accept_e)
                                .expect("submission queue is full");
                            ring.submission()
                                .push(&read_e)
                                .expect("submission queue is full");
                        }
                    },
                    Ctx::Echo(echo_ctx) => {
                        match echo_ctx.state { 
                            EchoState::Read => {
                                assert!(cqe.result() >= 0, "read error: {}", cqe.result());
                                let nread = cqe.result();
                                println!("read {} bytes", nread);
                                if nread == 0 {
                                    nix::unistd::close(echo_ctx.fd as _)?;
                                    fd_remove = Some(echo_ctx.fd);
                                } else {
                                    echo_ctx.state = EchoState::Write;
                                    echo_ctx.start = 0;
                                    echo_ctx.end = nread as _;
                                    let write_e = opcode::Write::new(types::Fd(echo_ctx.fd as _), echo_ctx.buf.as_mut_ptr(), echo_ctx.buf_size() as _)
                                        .build()
                                        .user_data(echo_ctx.fd as _);
                                    unsafe {
                                        ring.submission()
                                            .push(&write_e)
                                            .expect("submission queue is full");
                                    }
                                }
                            }
                            EchoState::Write => {
                                assert!(cqe.result() >= 0, "write error: {}", cqe.result());
                                let nwrite = cqe.result();
                                println!("written {} bytes", nwrite);
                                if echo_ctx.start + nwrite as usize >= echo_ctx.end {
                                    echo_ctx.state = EchoState::Read;
                                    echo_ctx.start = 0;
                                    echo_ctx.end = 0;
                                    let read_e = opcode::Read::new(types::Fd(echo_ctx.fd as _), echo_ctx.buf.as_mut_ptr(), echo_ctx.buf.len() as _)
                                        .build()
                                        .user_data(echo_ctx.fd as _);
                                    unsafe {
                                        ring.submission()
                                            .push(&read_e)
                                            .expect("submission queue is full");
                                    }
                                } else {
                                    echo_ctx.start += nwrite as usize;
                                    let write_e = opcode::Write::new(types::Fd(echo_ctx.fd as _), echo_ctx.buf.as_mut_ptr(), echo_ctx.buf_size() as _)
                                        .build()
                                        .user_data(echo_ctx.fd as _);
                                    unsafe {
                                        ring.submission()
                                            .push(&write_e)
                                            .expect("submission queue is full");
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if let Some(fd) = fd_remove {
                fd_ctx.remove(&fd);
            }
        }
    }

    Ok(())
}

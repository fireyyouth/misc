use mio::event::Event;
use mio::net::{TcpListener, TcpStream};
use mio::{Events, Interest, Poll, Registry, Token};
use std::collections::HashMap;
use std::time::Duration;
use std::io;
use std::error::Error;
use std::os::fd::AsRawFd;
use std::thread;
use socket2::{Socket, Domain, Type};
use std::sync::atomic::AtomicUsize;
use std::sync::{Arc, Mutex};
use std::os::fd::FromRawFd;

const WORKER: usize = 8;

fn main() -> Result<(), Box<dyn Error>> {
    let mut stat = Vec::new();
    for i in 0..WORKER {
        stat.push(Arc::new(AtomicUsize::new(0)));
    }

    let mut listener_holder = None;

    let args: Vec<String> = std::env::args().collect();
    if args.len() >= 2 && args[1].starts_with("r") {
        println!("multiple listening fd with reuseport");
        let addr: std::net::SocketAddr = "0.0.0.0:8000".parse()?;
        let addr = addr.into();
        for i in 0..WORKER {
            let stat = Arc::clone(&stat[i]);
            let socket = Socket::new(Domain::IPV4, Type::STREAM, None)?;
            socket.set_reuse_port(true)?;
            socket.bind(&addr)?;
            socket.listen(128)?;
            let listener: std::net::TcpListener = socket.into();
            thread::spawn(move || {
                work(i, stat, TcpListener::from_std(listener));
            });
        }
    } else {
        println!("shared listening fd");
        let listener = std::net::TcpListener::bind("0.0.0.0:8000")?;
        let fd = listener.as_raw_fd();
        for i in 0..WORKER {
            let stat = Arc::clone(&stat[i]);
            thread::spawn(move || {
                let listener = unsafe { std::net::TcpListener::from_raw_fd(fd) };
                work(i, stat, TcpListener::from_std(listener));
            });
        }
        listener_holder = Some(listener);
    }

    loop {
        thread::sleep(Duration::from_secs(1));
        let mut stage = Vec::new();
        let mut total = 0;
        for i in 0..WORKER {
            stage.push(stat[i].load(std::sync::atomic::Ordering::SeqCst));
            total += stage[stage.len() - 1];
        }
        println!("{}: {:?}", total, stage);
    }
    Ok(())
}

fn work(worker: usize, stat: Arc<AtomicUsize>, mut listener: TcpListener) {
    let mut poll = Poll::new().unwrap();
    let mut events = Events::with_capacity(128);

    const SERVER: Token = Token(0);
    poll.registry().register(&mut listener, SERVER, Interest::READABLE).unwrap();

    let mut conn_map = HashMap::new();
    loop {
        poll.poll(&mut events, Some(Duration::from_millis(100))).unwrap();
    
        for event in events.iter() {
            match event.token() {
                SERVER => loop {
                    match listener.accept() {
                        Ok((connection, address)) => {
                            stat.fetch_add(1, std::sync::atomic::Ordering::SeqCst);
                            conn_map.insert(connection.as_raw_fd(), connection);
                        },
                        Err(ref err) if would_block(err) => break,
                        Err(err) => panic!("serious accept error {:?}", err),
                    }
                }
                _ => {}
            }
        }
    }
}

fn would_block(err: &io::Error) -> bool {
    err.kind() == io::ErrorKind::WouldBlock
}

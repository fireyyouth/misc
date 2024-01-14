use tokio::time::{sleep, Duration};
use std::rc::Rc;
use std::cell::RefCell;

async fn work(n: usize) {
    let local = tokio::task::LocalSet::new();
    local.run_until(async {
        let conn = Rc::new(RefCell::new(Vec::new()));
        for _ in 0..n {
            let conn = Rc::clone(&conn);
            tokio::task::spawn_local(async move {
                let stream = tokio::net::TcpStream::connect("127.0.0.1:8000").await.unwrap();
                conn.borrow_mut().push(stream);
            });
        }
        loop {
            sleep(Duration::from_secs(1)).await;
        }
    }).await;
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let rt = tokio::runtime::Builder::new_current_thread().enable_io().enable_time().build().unwrap();
    rt.block_on(work(args[1].parse().unwrap()));
}


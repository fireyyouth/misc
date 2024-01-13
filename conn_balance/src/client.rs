use tokio::time::{sleep, Duration};
async fn work() {
    let mut conn = Vec::new();
    for _ in 0..1000 {
        conn.push(tokio::net::TcpStream::connect("127.0.0.1:8000").await.unwrap());
    }
    loop {
        sleep(Duration::from_secs(1)).await;
    }
}

fn main() {
    let rt = tokio::runtime::Builder::new_current_thread().enable_io().enable_time().build().unwrap();
    rt.block_on(work());
}


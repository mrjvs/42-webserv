const net = require('net');
const s = net.Socket();
const sleep = (sec) => new Promise((r) => setTimeout(()=>r, sec*1000));
const exit = () => {
    console.log('=> CLOSED MANUALLY');
}
const send = (d) => {
    console.log(`OUT => '${d}'`);
    s.write(d);
}
s.on('data', (d) => {console.log(`Inc => '${d.toString()}'`)});
s.on('close', () => {
    console.log('=> CLOSED EXT');
    process.exit();
})

s.on('connect', async () => {
    //send('POST / HTTP/1.1\r\nHost: localhost:8080\r\nConnection: keep-alive\r\nTRANSFER_ENCODING: chunked\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 11_1_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.182 Safari/537.36\r\nAccept-Language: nl-NL,nl;q=0.9,en-US;q=0.8,en;q=0.7\r\n\r\nBODYBODYBODY\r\nC\r\n');
    send('BODYBODY\r\n0\r\n');
    await sleep(1);
    // exit();
})

s.connect(8080);
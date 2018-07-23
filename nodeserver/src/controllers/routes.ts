import * as app from 'express';
import * as io from 'socket.io';

import CONFIG from '../config';

const router: app.Router = app.Router();
let ioServer: any = io(CONFIG.IO_PORT),
    sock : any = null,
    isMsgTimeout : boolean = false;

ioServer.on('connection', (socket) => {
   if(sock) {
       sock.disconnect();
       sock = null;
   }

   sock = socket;

   sock.on('disconnect', () => {
       console.log('Socket was disconnected');
    });

   console.log('Socket was connected');
});

setInterval(() => {
    if(sock && isMsgTimeout) {
        sock.emit('measure', {timeout: isMsgTimeout});
    }
}, CONFIG.MESSAGE_TIMEOUT / 2);

let timeout: any = -1;
router.post('/', (req: app.Request, res: app.Response) => {
    res.status(200);

    if(timeout !== -1)
        clearTimeout(timeout);

    timeout = setTimeout(() => {
        isMsgTimeout = true;
    }, CONFIG.MESSAGE_TIMEOUT);
    isMsgTimeout = false;

    (req.body)['timeout'] = isMsgTimeout;
    console.log(req.body);

    if(sock)
      sock.emit('measure', req.body);
});

export default router;

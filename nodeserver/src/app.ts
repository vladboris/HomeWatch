import * as express from 'express';
import * as compression from 'compression';
import * as bodyParser from 'body-parser';

import router from './controllers/routes';

class App {
    public app: express.Application;

    constructor() {
        this.app = express();
        this.config();
        this.routes();
    }

    private config(): void {
        this.app.use(compression());
        this.app.use(bodyParser.urlencoded({extended: true}));
        this.app.use(bodyParser.json());
    }

    private routes(): void {
        this.app.use('/', router);
    }
}

export default new App().app;

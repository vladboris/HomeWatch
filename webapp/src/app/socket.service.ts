import {
  Injectable,
} from '@angular/core';
import { environment } from '../environments/environment';
import {
  Observable
} from 'rxjs';

import * as io from 'socket.io-client';

@Injectable()
export class SocketService {

  private socket: any = null;
  private isConnected: boolean = false;
  private url: string;
  private observable$: Observable<any>;

  constructor() {
  }

  create(url) {
    if(!this.socket) {
      this.url = url;

      this.socket = io(this.url, {autoConnect: false});
    }
  }

  emit(event: string, data: any): void {
    if(this.isConnected) {
      this.socket.emit(event, data, (ack) => {
        console.log('Emit ack: ' + ack);
      });
    }
  }

  connectOn(...eventNames: string[]): Observable<any> {
    this.observable$ = new Observable<any>( (observer) => {
      if(this.socket && !this.isConnected) {
        this.socket.connect();

        this.socket.on("connect", () => {
          this.isConnected = true;
          observer.next({conStatus: this.isConnected});
          observer.next({conInfo: "Socket Connected"});
        });

        this.socket.on("disconnect", () => {
          this.isConnected = false;
          observer.next({conStatus: this.isConnected});
          observer.next({conInfo: "Socket Disconnected"});
        });

        this.socket.on("error", (error: string) => {
          if (!environment.production)
            console.log(`ERROR: "${error}" (${this.url})`);

          observer.next({conInfo: `Socket ERROR: "${error}" (${this.url})`});
        });

        if(eventNames !== undefined) {
          eventNames.forEach((evName) => {
            this.socket.on(evName, (message) => {
              const msg = message; // JSON.parse(message);
              msg['_evName'] = evName;

              observer.next(msg);
              observer.next({conInfo: "Socket message accepted"});
            });
          });
        }

        return () => { // when .unsubscribe() called
          this.socket.disconnect();
          observer.next({conInfo: "Stream unsubscribed"});
        }

      } else
        observer.next({conInfo: "Socket connection already exist"});
    });

    return this.observable$;
  }

  disconnect() {
    if(this.isConnected) {
      this.socket.disconnect();
    }
  }

  close() {
    if(this.socket) {
      this.disconnect();
      this.socket = null;
    }
  }

  getConnectionStatus() : boolean {
    return this.isConnected;
  }

  getObservable(): Observable<any>{
    return this.observable$;
  }

  /*on(event_name) {
   return new Observable<any>(observer => {
   if(this.isConnected) {
   //....
   } else {
   observer.next({conInfo: 'Connection doesnt\'t exist'});
   }
   });
   }*/
}

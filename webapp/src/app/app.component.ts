import {
  Component,
  OnInit,
  OnDestroy
} from '@angular/core';
import {SocketService} from './socket.service';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent implements OnInit, OnDestroy {
  connection: any;
  connectionStatus: boolean = false;

  public title = 'app';
  public isData: boolean = false;

  public humi: any = '00.00';
  public cels: any = '00.00';
  public fahr: any = '00.00';

  constructor(private socket: SocketService) {
  }

  ngOnInit() {
    this.socket.create('http://192.168.1.6:4000');

    if (!this.socket.getConnectionStatus()) {
      this.connection = this.socket.connectOn('measure')
        .subscribe((data) => {
          console.log(data);

          if (data['_evName'] !== undefined &&
              data['_evName'] == 'measure') {
            this.parseData(data);
          }
        });
    }
  }

  ngOnDestroy() {
    this.connection.unsubscribe();
  }

  private parseData(data: any): void {
    const isNan = (data.humi == 'nan' ||
                   data.cels == 'nan' ||
                   data.fahr == 'nan');

    if (isNan || data.timeout) {
      this.isData = false;
    } else {
      this.isData = true;

      this.humi = data.humi;
      this.cels = data.cels;
      this.fahr = data.fahr;
    }
  }
}

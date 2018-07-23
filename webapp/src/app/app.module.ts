import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import {
  LocationStrategy,
  HashLocationStrategy,
  // PathLocationStrategy
} from '@angular/common';
import {APP_BASE_HREF} from '@angular/common';

import { SocketService } from './socket.service';
import { AppComponent } from './app.component';

@NgModule({
  declarations: [
    AppComponent
  ],
  imports: [
    BrowserModule
  ],
  providers: [
    { provide: APP_BASE_HREF, useValue: '/' },
    { provide: LocationStrategy, useClass: HashLocationStrategy }, /*PathLocationStrategy*/
    SocketService
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }

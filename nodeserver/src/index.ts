import app from './app';
import CONFIG from './config';

process.env.PORT = CONFIG.HTTP_PORT.toString();

 const port = (process.env.PORT || CONFIG.HTTP_PORT);

 app.listen(port, (err) => {
   if (err)
     return console.log(err);

   return console.log(`server is listening on ${port}`);
 });

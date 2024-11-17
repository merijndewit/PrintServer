# Websocket commands

Here you will find a list of all the websocket commands that can be send from the server (esp) to the client. These commands are optimised to be as small as possible and lightweight for the server.

 > format: {id};{parameter} (multiple parameters are seperated with a ';') 

 > Note: for the format see the sprintf documentation

| Command id | Parameters |  format | range  |
|  --------  |   -------  | ------- | ------ |
| 10  |  (completed upload percentage)   | (%.2f%%) | 0-100% |
| 20  | (sd connected) | (%i) | 0-1 |
| 21  | (available memory in gb)    | (%.1f) | - |
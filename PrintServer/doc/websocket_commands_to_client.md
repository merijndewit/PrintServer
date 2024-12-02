# Websocket commands

Here you will find a list of all the websocket commands that can be send from the server (esp) to the client. These commands are optimised to be as small as possible and lightweight for the server.

 > format: {id};{parameter} (multiple parameters are seperated with a ';')

 > Note: for the format see the sprintf documentation

| Command id | Parameters |  format | range  |
|  --------  |   -------  | ------- | ------ |
| 10  |  (completed upload percentage)   | (%.2f%%) | 0-100% |
| 11  | (is printing)    | (%i) | 0-1 |
| 20  | (sd connected) | (%i) | 0-1 |
| 21  | (available memory in gb)    | (%.1f) | - |
| 22  | (file count)    | (%i) | 0-32 |
| 23  | (sd speed)    | (%i) | - |
| 24  | (stored file)    | (%s) | 0-128 |
| 30  | (minutes online)    | (%.2f) | - |
| 30  | (total prints printed)    | (%i) | - |
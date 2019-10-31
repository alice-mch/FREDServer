The ALFRED API library provides all resources for building FRED modules using a 
simple class inheritance with minimum need of extending provided classes. 
A powerful tool provided by the library is the ALFRED connection system, which 
allows automatic message sharing between selected DIM services and commands 
(e.g.  after a specified DIM command receives data, those data can be 
automatically published in another DIM service with no need of additional 
programming, just with the use of single API connect function).   
The library also handles possible error conditions, e.g. within one server 
there cannot be two services or commands with the same name.  Also when a server 
is being created, the library checks whether there is a server with the same 
name registered within the DIM Name Server.
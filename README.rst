============================
Javascript-C++ Terminal
============================
.. image:: https://secure.travis-ci.org/berenm/anyterm.png?branch=master
    :alt: Build Status
    :target: https://travis-ci.org/berenm/anyterm


INTRO
````````````````````````````

Anyterm is HTML5/Javascript interface communicating remotely via HTTP(S) protocol to a server-side bash (or similar) process.
It is meant to be used for remote accessing a distant computer from a restricted environment, where HTTP is the only allowed protocol.

I originally forked the project from Phil Endecott's source code (http://anyterm.org), available under GPL license, but I finally rewrote the majority of the source code. The name is probably going to change in the near future, when the project will be stable enough.


USAGE
````````````````````````````

Anyterm comes in two part:

- A server-side process, ``anytermd``, in charge of managing user-sessions and run the shell child processes and the wrapping terminal emulation layer (implemented using awesome GNOME's libvte).
- An HTML page, with JQuery javascript code, in charge of capturing key inputs and translating them to ANSI control sequences that will be fed to the ``anytermd`` child processes. 

``Anytermd`` is not meant to be accessed directly from the network. The server does not do any kind of authentication or security enforcement. It focuses exclusively on session managment and interface with the terminal emulator. Instead, ``anytermd`` should be used behind a full-featured web server such as nginx, which will be configured to serve the static content (html page, css, and javascript), handle encryption (with HTTPS), and handle user authentication.

Current implementation can be tested with the following nginx configuration (It's a simple proxy, apache can do the same):

.. code:: nginx

  server {
    listen 127.0.0.1:8000;

    root <path-to-anyterm-source>/anyterm/www;
    index anyterm.html;

    location / {
      try_files $uri $uri/ anyterm.html;
    }

    location /anytermd {
      proxy_pass http://localhost:8088;
    }
  }


Running ``anytermd`` is easy (for dependencies, look at the installed packages in .travis.yml):

.. code:: bash

  mkdir build
  cd build
  cmake ..
  make
  ./anytermd


Then, interface can be accessed at http://127.0.0.1:8000. Chrome and Firefox browsers should be supported.


CONFIGURATION
````````````````````````````
For now, there is none, ``anytermd`` offers access to a bash instance, started with the same user as the ``anytermd`` process. Might be possible later on.


COPYING INFORMATION
````````````````````````````

 Distributed under the Boost Software License, Version 1.0.

 See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

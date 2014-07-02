Termidor: a door to your home terminal
=======================================
.. image:: https://secure.travis-ci.org/berenm/termidor.png?branch=master
    :alt: Build Status
    :target: https://travis-ci.org/berenm/termidor

.. image:: https://coveralls.io/repos/berenm/termidor/badge.png?branch=master
    :alt: Coverage Status
    :target: https://coveralls.io/r/berenm/termidor

.. image:: http://stillmaintained.com/berenm/termidor.png
    :alt: Still Maintained?
    :target: http://stillmaintained.com/berenm/termidor


INTRO
````````````````````````````

Termidor is a HTML5/Javascript interface communicating remotely via HTTP(S) protocol with a server-side bash (or similar) process.
It is meant to be used for remote access from a restricted environment, where HTTP is the only allowed protocol.

USAGE
````````````````````````````

Termidor comes in two part:

- A server-side process, ``termidord``, in charge of managing user-sessions and running the shell child processes and the wrapping terminal emulation layer (implemented using the awesome GNOME's libvte).
- An HTML page, with JQuery javascript code, in charge of capturing key inputs and translating them to ANSI control sequences that will be fed to the ``termidord`` child processes. 

``termidord`` is not meant to be accessed directly from the network. The server does not support any kind of authentication or security enforcement.
It is focused exclusively on session managment and interface with the terminal emulator.
Instead, ``termidord`` should be used behind a full-featured web server such as nginx, which will be configured to serve the static content (html page, css, and javascript), handle encryption (with HTTPS), and handle user authentication.

Current implementation can be tested with the following nginx configuration (It's a simple proxy, apache can do the same):

.. code:: nginx

  server {
    listen 127.0.0.1:8000;

    root <path-to-termidor-source>/termidor/www;
    index termidor.html;

    location / {
      try_files $uri $uri/ termidor.html;
    }

    location /termidor {
      proxy_pass http://localhost:8088;
    }
  }


Running ``termidord`` is easy (for dependencies, look at the installed packages in .travis.yml):

.. code:: bash

  mkdir build
  cd build
  cmake ..
  make
  ./termidord


Then, interface can be accessed at http://127.0.0.1:8000. Chrome and Firefox browsers should be supported.


CONFIGURATION
````````````````````````````
For now, there is none, ``termidord`` spawns a ``/bin/bash -l`` instance, started with the same user as the ``termidord`` process. Might be possible later on.


COPYING INFORMATION
````````````````````````````

 Distributed under the Boost Software License, Version 1.0.

 See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

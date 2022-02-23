<br />
<p align="center">
    <a href="https://github.com/dev2alert/node-samp">
        <img src="https://raw.githubusercontent.com/dev2alert/node-samp/main/big-logo.png" width="250px" />
    </a>
</p>
<p align="center">
    Runtime environment Node.js in SA-MP.
</p>
<p align="center">
    <img src="https://img.shields.io/static/v1?label=version&message=1.0.0&color=yellow" />
    <a href="https://github.com/dev2alert/node-samp-plugin/blob/main/LICENSE"><img src="https://img.shields.io/github/license/dev2alert/node-samp-plugin" /></a>
</p>

## Description
<p>
    SA-MP plugin for <a href="https://github.com/dev2alert/node-samp">NodeSamp</a>.
</p>

## Documentation
<p>
    Documentation is <a href="https://github.com/dev2alert/node-samp/wiki">here</a>.
</p>

## Building
Windows prerequisites:
<ul>
    <li><a href="https://visualstudio.microsoft.com/ru/downloads/">Visual Studio 2019</a> (C / C++)</li>
    <li><a href="https://cmake.org/">CMake</a> >= <code>3.20.6</code></li>
    <li><a href="https://www.gnu.org/software/make/">GNU Make</a> >= <code>4.3</code></li>
</ul>

Linux prerequisites:
<ul>
    <li><a href="https://gcc.gnu.org/">GCC / G++</a> >= <code>9.3.0</code></li>
    <li><a href="https://cmake.org/">CMake</a> >= <code>3.16.3</code></li>
    <li><a href="https://www.gnu.org/software/make/">GNU Make<a> >= <code>4.2.1</code></li>
</ul>

Linux prerequisites in other OS:
<ul>
    <li><a href="https://www.docker.com/">Docker</a> >= <code>20.10.8</code></li>
</ul>

For Windows:<br />
```
make build-win32-start
make -B build
```
For Linux<br />
```
make build-start
make -B build
```

For Linux in other OS:<br />
```
make build-linux-start
make build-linux
```

## License
<p>
    <a href="./LICENSE">MIT</a>
</p>

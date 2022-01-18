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
    <li>Visual Studio 2019 (C / C++).</li>
    <li>CMake 3.20.6.</li>
    <li>GNU Make 4.3.</li>
</ul>

Linux prerequisites:
<ul>
    <li>GCC / G++ 9.3.0.</li>
    <li>CMake 3.16.3.</li>
    <li>GNU Make 4.2.1.</li>
</ul>

For Windows:<br />
```
cd ./build && cmake .. -A Win32 && cd ..
make -B build
```
For Linux:<br />
```
cd ./build; cmake ..; cd ..
make -B build
```

## License
<p>
    <a href="./LICENSE">MIT</a>
</p>
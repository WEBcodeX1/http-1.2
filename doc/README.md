# Internal Class Reference / Interface Documentation

The following documentation will be rendered by Doxygen and a Python
Doxygen-Sphinx bridge (breathe).

Online https://docs.webcodex.de/developer/doxygen/ and https://docs.webcodex.de/developer/sphinx/.

# 1. Prerequisites

The following packages (Ubuntu 22.04) need to be installed:

```bash
# apt-get install doxygen python3-sphinx python3-sphinx-rtd-theme python3-breathe
```

# 2. Continuous Integration

In future releases this process will be integrated in CI systems and
triggered regulary.

# 3. Render Documentation

```bash
# doxygen
# sphinx-build -b html -Dbreathe_projects.falconas=doc_render/xml . doc_render/sphinx/
```

# 4. Rendered Content

Rendererd Documentation can be found at the following locations.

## 4.1. Doxygen

`./doc_render/html/index.html`.

## 4.2. Sphinx

`./doc_render/sphinx/index.html`.

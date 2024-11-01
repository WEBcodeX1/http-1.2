# 1. Internal Class Reference / Interface Documentation

The following documentation will be rendered by Doxygen and a Python
Doxygen-Sphinx bridge (breathe).

# 2. Prerequisites

The following packages (Ubuntu 22.04) need to be installed:

```bash
# apt-get install doxygen python3-sphinx python3-sphinx-rtd-theme python3-breathe
```

# 3. Continuous Integration

In future releases this process will be integrated in CI systems and
triggered regulary.

# 4. Render Documentation

```bash
# doxygen
# sphinx-build -b html -Dbreathe_projects.falconas=doc_render/xml . doc_render/sphinx/
```

# 5. Rendered Content

Rendererd Documentation can be found at the following locations.

## 5.1. Doxygen

`./doc_render/html/index.html`.

## 5.2. Sphinx

`./doc_render/sphinx/index.html`.

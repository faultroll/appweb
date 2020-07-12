Deploy Appweb Sample
===

This sample demonstrates the commands to use to deploy Appweb files to a staging directory.

Requirements
---
<<<<<<< HEAD
* [Appweb](https://embedthis.com/appweb/download.html)
* [MakeMe Build Tool](https://embedthis.com/makeme/download.html)
=======
* [Appweb](https://www.embedthis.com/appweb/download.html)
* [MakeMe Build Tool](https://www.embedthis.com/makeme/download.html)
>>>>>>> local

Steps:
---

1. Appweb must be built

<<<<<<< HEAD
    me 

2. Deploy 

    me --sets core,libs,esp --deploy dir
=======
    me

2. Deploy

    cd top-appweb-directory
    me --sets core,libs,esp --deploy /tmp/appweb
>>>>>>> local

This will copy the required Appweb files to deploy into the nominated directory.

Other sets include: 'web', 'service', 'utils', 'test', 'dev', 'doc', 'package'

Documentation:
---
<<<<<<< HEAD
* [Appweb Documentation](https://embedthis.com/appweb/doc/index.html)
* [Building Appweb with MakeMe](https://embedthis.com/appweb/doc/source/me.html)
=======
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [Building Appweb with MakeMe](https://www.embedthis.com/appweb/doc/source/me.html)
>>>>>>> local

See Also:
---
* [min-server - Minimal server configuration](../min-server/README.md)
* [simple-server - Simple one-line embedding API](../simple-server/README.md)

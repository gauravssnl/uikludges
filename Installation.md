## By building ##

Symbian platform security makes it difficult to distribute development binaries. Thus, we recommend you to build this thing your self.

By default:

  * uikludges is built with all developer certificate capabilities assigned to DLL files

  * uikludges uses protected UID range (UIDs reserved for Red Innovation Ltd.)

To build your own version

  * Get a developer certificate, Series 60 SDK, Python for Series 60 source code

  * Reserve UIDs for yourself

  * Fix UID references in source code

  * Rebuild

  * Sign SIS with your own certificate

  * Install to the phone

### Tutorial ###

See [this tutorial](http://wiki.opensource.nokia.com/projects/PyS60_extensions_creation_using_Carbide) for more information how to create your own PyS60 Native extensions using Carbide C++.



## By SIS hacking ##

Other way is to get Ensymble and other [SIS manipulation tools](http://discussion.forum.nokia.com/forum/showthread.php?t=129040&highlight=unsis) and change capabilities and UIDs directly in the binary file.
There are actually 3 things that need to be considered when syncing your iPhone with the server.

  * The media saved in the Documents directory for the phone
  * The preferences saved on the phone
  * The stuff saved on the server

## App Startup ##

When the iPhone app starts up, the first thing it does is look in the documents directory for any videos or images and adds them to the interface.  If information exists in the preferences, the interface is updated.

## The Snycing Process ##

Then, the information that has been added to the interface is sent to the server. Anything that has changed is added to the existing entries, and any additional entries are added.  Then the whole mess is sent back to the iphone, which replaces everything in the interface with the stuff from the server.
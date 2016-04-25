This is how the current server works, and it isn't very web interface-friendly. It is iPhone-specific.

# Uploading #

## Post Vars ##
  * **action** - (required) - in this case, should equal 'upload'
  * **media\_id** - (required) - This is currently generated on the device, but shoulr probably be generated on the server and returned to the device.  It is currently [version of IP address without dots](hex.md)-[unixtime seconds](hex.md)-[unixtime milliseconds part](hex.md)  It is used to save the file
  * **title** - (optional)
  * **udid** - (required) - unique device ID. This is currently based on the iphone UDID, but should be generalized for other devices.  I suppose it could be
  * **device\_name** - (optional) - The name of the device (pretty much useless)

## Files ##
  * **file** - Either a JPEG or a video (mime type 'image/jpeg', 'image/png', or ''video/**' are accepted)
  ***device\_token**- for iPhone, the device token allows the server to send PUSH notifications**

## Response ##

Response is in JSON format.  It is an object with at least 1 member: 'status'

If 'status' is 'ok', everything went fine.

```
{"status":"ok"}
```

If status is 'error', than there will be another member called 'errors', which is an array of error objects.

Error objects have 2 members: 'code' and 'message'.  The first is meant to be machine-readable, and the second is for humans.  These are the current codes:

  * UNREADABLE\_TOKEN
  * NO\_ACTION
  * NO\_UDID
  * NO\_MEDIA\_ID
  * PROCESSING\_ERROR
  * NO\_FILE\_POSTED
  * UPLOAD\_DIR
  * NO\_MEDIA\_ID
  * MEDIA\_ID\_EXISTS
  * UNKNOWN\_IMAGE\_FORMAT
  * UNKNOWN\_UPLOAD\_FORMAT
  * MOVE\_UPLOAD\_FAILED
  * SQL\_ERROR
  * NO\_TOKEN

# Syncing #

{
"status":"ok",
"media":[{"original":"616795f-4c5b2a54-f7c22dcc.jpg","media\_id":"616795f-4c5b2a54-f7c22dcc","errors":[.md](.md),"media\_type":"image","user\_id":"1","status":"done","in\_database":true,"title":"Image 1","download\_link":"http:\/\/unlogo.org\/processed\/616795f-4c5b2a54-f7c22dcc.jpg}","id":1,"date\_uploaded":"2010-08-05 14:52:59"}]
}


# Downloading #
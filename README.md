# dvb_capture

Exercise in processing MPEG Transport Streams broadcast over DVB-T

## Dependencies

- Windows (Testing on Windows 7)

- DVB-T Tuner 
  - Testing with "[August DVB-T210](https://www.amazon.co.uk/gp/product/B008D8K50Q)" USB Freeview HD TV Tuner 
  
- DirectShow '[base classes](https://msdn.microsoft.com/en-us/library/windows/desktop/dd318238(v=vs.85).aspx)', as provided with Windows SDK 7.1 


## Goals

- ~~Learn how to work with a DVB-T tuner with Microsoft TV Technologies + DirectShow~~
  - [Microsoft TV Technologies Internals](https://msdn.microsoft.com/en-us/library/ms787280(VS.85).aspx)
  - [Microsoft Broadcast Device Architecture](https://msdn.microsoft.com/en-us/library/windows/desktop/dd693007(v=vs.85).aspx)

- ~~Learn how to stream an MPEG Transport Stream from a DVB-T Tuner~~

- Learn how to filter an MPEG Transport Stream in to multiple services (/channels)

- Learn how to filter elementary streams for each service, so they can be muxed for playback offline

- Learn how to process 


## References

- DirectShow DVB-T Tuner graph [https://groups.google.com/forum/#!topic/microsoft.public.win32.programmer.directx.video/Hvy8Vl0bWWY](requires MPEG Demux + TIF to work), even though we're not using them
  
- [https://ukfree.tv/transmitters/tv/Crystal_Palace](Frequencies for UK/FreeView transmitters)
  
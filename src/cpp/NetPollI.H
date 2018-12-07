#ifndef _NETPOLLI
#define _NETPOLLI

#include "NetChannel.H"
#include <vector>
#include <memory>

template <typename PollTraits>
class PollerT {
  public:
    typedef typename PollTraits::ChannelPtr  ChannelPtr;
    typedef typename PollTraits::ChannelList ChannelList;

    PollerT()=default;
    virtual ~PollerT()=default;
    //no copy
    PollerT(const PollerT&)=delete;
    // can move
    PollerT(PollerT&&) = default;

    virtual bool valid() const = 0;
    virtual void addChannel(const ChannelPtr&) = 0;
    virtual void rmChannel(const ChannelPtr&) = 0;
    virtual int multiplex(int timeout) = 0;
    virtual void pendingList(ChannelList &) = 0;

};

class VectorPollTraits {
  public:
    typedef std::shared_ptr<NetChannel>  ChannelPtr;
    typedef std::vector<ChannelPtr>      ChannelList;
};

#endif


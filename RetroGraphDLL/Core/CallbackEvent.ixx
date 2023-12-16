export module RG.Core:CallbackEvent;

import "EventppHeaderUnit.h";

namespace rg {

export template<class ...Args>
class CallbackEvent {
public:
    using CallbackList = eventpp::CallbackList<void(Args...)>;
    using Callback = CallbackList::Callback;
    using Handle = CallbackList::Handle;

    [[nodiscard]] Handle attach(Callback callback) const {
        return m_callbackList.append(callback);
    }

    void detach(Handle handle) const {
        m_callbackList.remove(handle);
    }

    void raise(Args... args) {
        m_callbackList(args...);
    }

private:
    mutable CallbackList m_callbackList;
};

} // namespace rg

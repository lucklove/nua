#pragma once

class ScopeGuard
{
public:
    explicit ScopeGuard(std::function<void()> on_exit)
        : on_exit_(on_exit), dismissed_(false)
    {}

    ~ScopeGuard()
    {
        if (!dismissed_)
        {
            on_exit_();
        }
    }

    void dismiss(bool dismissed=true)
    {
        dismissed_ = dismissed;
    }

    ScopeGuard(ScopeGuard const&) = delete;
    ScopeGuard& operator=(ScopeGuard const&) = delete;

private:
    std::function<void()> on_exit_;
    bool dismissed_;
};

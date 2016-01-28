// clang format off
#include <condition_variable>
#include <cstdlib>
#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include <wayland-client.h>
#include "dk_utils/make_unique.hpp"
#include "nullbackend.hpp"
#include "server/waylandserver.hpp"
#include "testclient.hpp"
// clang fomrat on

using namespace std;

class WaylandServerFixture : public testing::Test
{
public:
    WaylandServerFixture() : mSocketName("test_socket"), mBackend(nullptr) {
        std::lock_guard<mutex> g(mMutexIncreaseSocketNumber);
        mSocketName.append(to_string(sSocketNumber++));
    }

    void SetUp() {
        auto b = make_unique<NullBackend>(320, 480);
        mBackend = b.get();
        mThreadServer = thread(
            [](unique_ptr<IDisplayBackend> backend, condition_variable& c, const string& s) {
                WaylandServer wl(move(backend), s);
                c.notify_one();
                wl.run();
            },
            move(b), ref(mConditionServerReady), mSocketName);
        unique_lock<mutex> lk(mMutexServerReady);
        mConditionServerReady.wait(lk);
    }

    void TearDown() {
        mBackend->finish();
        mThreadServer.join();
    }

    ~WaylandServerFixture() {
        // cleanup any pending stuff, but no exceptions allowed
    }

    const string& getSocketName() const {
        return mSocketName;
    }

private:
    static int sSocketNumber;
    mutex mMutexIncreaseSocketNumber;
    string mSocketName;
    thread mThreadServer;
    mutex mMutexServerReady;
    condition_variable mConditionServerReady;
    NullBackend* mBackend;
};

int WaylandServerFixture::sSocketNumber = 0;

TEST(WaylandServer, simple_connection) {
    shared_ptr<client::TestClient> wc;
    EXPECT_ANY_THROW(wc = make_shared<client::TestClient>("non_existing_socket"));
}

TEST_F(WaylandServerFixture, simple_connection) {
    shared_ptr<client::TestClient> wc;
    EXPECT_NO_THROW(wc = make_shared<client::TestClient>(getSocketName()));
    EXPECT_NE(nullptr, wc->getWlDisplay());
}

TEST_F(WaylandServerFixture, destroy_surface) {
    client::TestClient wc(getSocketName());
    wl_surface* wl = wc.createSurface();
    wl_surface_destroy(wl);
    wc.commitChangesToServer();
}

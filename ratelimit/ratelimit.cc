#include <tuple>
#include <stdint.h>
typedef uint64_t u64;

typedef std::tuple<u64, u64, u64> Tuple;

struct Limiter {
    const u64 period;
    u64 slot[2];
    u64 last_time;

    Limiter(u64 p): slot(), period(p), last_time(0) {}
    
    void update(u64 t, u64 cnt) {
        u64 pid = t / period;
        u64 last_pid = last_time / period;
        if (pid + 1 < last_pid) {
            return;
        }
        if (pid >= last_pid + 2) {
            slot[pid & 1] = cnt;
            slot[1 - (pid & 1)] = 0;
        } else if (pid == last_pid + 1) {
            slot[pid & 1] = cnt;
        } else { // pid == last_pid || pid + 1 == last_pid
            slot[pid & 1] += cnt;
        }
        last_pid = pid;
    }
    u64 count() const {
        u64 last_pid = last_time / period;
        u64 prev_ratio = (period - (last_time % period + 1)) / period;
        return slot[last_pid & 1] + slot[1 - (last_pid & 1)] * prev_ratio;
    }

    void clear() {
        slot[0] = 0;
        slot[1] = 0;
    }

    Tuple dump() const {
        return std::make_tuple(slot[0], slot[1], last_time);
    }
    void set(Tuple &x) {
        slot[0] = std::get<0>(x);
        slot[1] = std::get<1>(x);
        last_time = std::get<2>(x);
    }
};


struct MasterLimiter {
    Limiter total;
    MasterLimiter(u64 period): total(period) {}
    void apply_local(Tuple data) {
        auto &[slot_0, slot_1, t] = data;
        total.update(t - 1, slot_0);
        total.update(t, slot_1);
    }
    Tuple dump_total() {
        return total.dump();
    }
    bool detect(u64 t) {
        total.update(t, 1);
        return total.count() > 3;
    }
};


struct SlaveLimiter {
    Limiter local, total_copy;
    SlaveLimiter(u64 period): local(period), total_copy(period) {}
    void apply_total(Tuple data) {
        total_copy.set(data);
    }
    Tuple clear_local() {
        auto r = local.dump();
        local.clear();
        return r;
    }
    bool detect(u64 t) {
        local.update(t, 1);
        return total_copy.count() > 3;
    }
};

int main() {
    MasterLimiter master(10);
    SlaveLimiter slave(10);

    auto local = slave.clear_local();
    master.apply_local(local);
    auto total = master.dump_total();
    slave.apply_total(total);
}


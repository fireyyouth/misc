struct SegTree {
    vector<int> heap;
    int size;
    int ih;
    SegTree(int s) {
        ih = ceil(log2(s));
        size = s;
        heap.resize(1 << (ih + 1));
    }
    void update(int i, int v) {
        int x = (1 << ih) - 1 + i;
        heap[x] = v;
        x = (x - 1) / 2;
        while (1) {
            heap[x] = max(heap[2 * x + 1], heap[2 * x + 2]);
            if (x == 0) {
                break;
            }
            x = (x - 1) / 2;
        }
    }
    int f(int x, int p, int q, int i, int j) {
        if (q < i || j < p) {
            return 0;
        }
        if (i <= p && q <= j) {
            return heap[x];
        }
        return max(
            f(2 * x + 1, p, (p + q) / 2, i, j),
            f(2 * x + 2, (p + q) / 2 + 1, q, i, j)
        );
    }
    int range_max(int i, int j) {
        return f(0, 0, (1 << ih) - 1, i, j);
    }
};

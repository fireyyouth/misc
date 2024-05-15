## sliding window ratelimit demo
### one rule on one node
maintain a timeline(@1 suppose we have unlimited memory, will opt later), with `period` as unit, each cell is a counter
```
| p0 | p1 | p2 | p3 | ....
```
whenever a request come, update corresponing counter
- now estimated access count = `current_counter + prev_counter * period - (t % period + 1)`

### support multi-node cluster
one center node maintain a timeline named `total`
each detect node maintain two seperate timelines, namely `local` and `total_copy`
whenever a request come, update `local`
after every T time (sync interval), detect node send changes of `local` since last sync to center node, center node add these changes to its time line, and return a full copy to `total_copy` (@2 suppose we have unlimited memory, will opt later)
- now each node can use the aggregated data, with delay in `[T, 2 * T]`, worst node see prev prev sync total + little something, best node see prev sync total (not good but somewhat acceptable)


### optimize memory
only maintain two counters for a timeline, nothing other than these two counter will be used. 

### support multi rule
at sync time, detect node send only those that are dirty from last sync, center node have to return all (sad, how to fix?)

## conclusion
sliding window is suitable for ratelimit cluster, the center node may be redis or custom server.
if we allow a bit delay, detect node can just read from center node when needed, and when syncing the center node does not need to return any data, this simplifies design and effectly remove the `total_copy` cache.

but if `total_copy` cache must be used, there is one serious problems:
1. center node have to return all rule data when sync
- an idea: center node can maintain a changelog like following
```
1: a b
2: c
3: a e
```
during sync, center node return data as well as version, at next sync time, detect node send this version, center node iterate changelog from this version  to  latest version and return changed items
just maintain changelog in recent 2 * T should be enough



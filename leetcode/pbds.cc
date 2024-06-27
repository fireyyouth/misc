#include <ext/pb_ds/assoc_container.hpp> 
#include <ext/pb_ds/tree_policy.hpp> 
using namespace __gnu_pbds; 

using Key = pair<int, int>;
using ordered_set = tree<Key, null_type, less<Key>, rb_tree_tag,tree_order_statistics_node_update>; 

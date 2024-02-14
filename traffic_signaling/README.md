### class Schedule

- represents a schedule for one intersection
- given a street_id and a current time, it should return the earliest next time when the light will be green for this street in `O(1)` time via `next_green` method

- `street_index_` $I$ - a mapping from absolute street id to relative index for this intersection
- `green_lights_` $G$ - time intervals when the light is green for each street indexed by `street_index_`

Now, suppose we have a schedule represented as a *permutation* of the relative street index $\pi = [3, 1, 2, 0]$, which describes the *order* of green light, and a *duration* of the green light for each street $l = [2, 0, 2, 1]$. Then, the following table shows the internal representation of the `green_lights_` $G$ for this schedule:

| $street\_id$\* | $I[street\_id]$ | $\pi[I[street\_id]]$ | $l[I[street\_id]]$ | $G[I[street\_id]]$ |
|:--------------:|:---------------:|:--------------------:|:------------------:|:------------------:|
| $4579$         | $0$             | $3$                  | $2$                | $[3, 5)$           |
| $279 $         | $1$             | $1$                  | $0$                | $(1, 1) = \empty$  |
| $31  $         | $2$             | $2$                  | $2$                | $[1, 3)$           |
| $158 $         | $3$             | $0$                  | $1$                | $[0, 1)$           |

\*The $street\_id$ in this example is arbitrary.
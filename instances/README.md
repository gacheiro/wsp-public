# Instances

This folder holds the instances used to test the methods. Every instance has a
**vertex file** and an **edge file**.

| Path | Description |
|------|-------------|
| `vertices.txt` | The vertex file shared by all simulated instances: 10 nodes (2 origins, 3 destinations, 5 ordinary nodes). |
| `g1/` | The original simulated instances from Sakuraba et al. (2016). |
| `g2/` | The `g1` instances with the road width (`u`) of every edge set to 1. |
| `g3/` | The `g1` instances with the road width (`u`) of every edge set to 2. |

Each simulated group has 88 instances (11 networks × 2 values of `r` × 4 values of `|B|`).

The Port-au-Prince (Haiti) instance used in the papers is not included, because it cannot be made public.

## Naming

An edge file name such as `1n10r40B15.txt` reads as follows:

- `1`: the network index (0 to 10)
- `n10`: 10 nodes
- `r40`: the sum of the repair times `r` over all edges (40 or 45)
- `B15`: the number of blocked edges (5, 10, 15 or 20)

Each simulated instance has 20 edges.

## Vertex file format

```
[nb. nodes]
[node id] [x coord] [y coord] [type] [p]
...
```

- `type`: `0` ordinary node, `1`, `2` or `3` origin (a work-troop base), `4` destination
- `p`: for an origin, the number of work-troops (WT) based there; for a destination, its population (demand)

## Edge file format

```
[nb. edges]
[edge id] [node i] [node j] [d] [u] [r]
...
```

- `d`: the length of the road
- `u`: the road width, which is the maximum number of work-troops that can work on the edge at the same time
- `r`: the repair time, in periods of one work-troop. An edge is blocked when `r > 0`

## Related folders

- `../cplex/` has the `.lp` models and CPLEX logs used to compute optimal solutions for the simulated instances.
- `../results/` has the results for the three groups. For group 1, the LCH results are the ones reported by Sakuraba et al. (2016).

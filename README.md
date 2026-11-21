# Metaheuristics for the Work-Troops Scheduling Problem

This repository has the code, instances and results of the paper:

> T. J. Barbalho, A. C. Santos, D. J. Aloise. **Metaheuristics for the work-troops scheduling problem.**
> *International Transactions in Operational Research*, 30(2):892–914, 2023.
> DOI: [10.1111/itor.12925](https://doi.org/10.1111/itor.12925)

A French version of this work was also published:

> T. J. Barbalho, A. C. Santos, D. J. Aloise. **Méthodes approchées pour le problème de planification du nettoyage de débris dans des réseaux urbains.**
> *Cahiers de la logistique*, 2:1–11, 2021 (series *Optimisation des opérations de déblaiement suite aux catastrophes en milieu urbain*).
> HAL: [hal-03355337](https://normandie-univ.hal.science/LOGISTIQUE/hal-03355337v1)

It was also presented at ROADEF 2022:

> T. J. Barbalho, A. C. Santos, D. J. Aloise. **Metaheuristics for cleaning debris in urban networks after major disasters.**
> *23ème congrès annuel de la Société Française de Recherche Opérationnelle et d'Aide à la Décision (ROADEF 2022)*, Villeurbanne - Lyon, France, February 2022.
> HAL: [hal-03596190](https://hal.science/hal-03596190)

## The problem

After a large disaster, such as the 2010 earthquake in Haiti, collapsed buildings
and debris block many roads. Relief teams cannot reach the people who need help
until the roads are open again. The **Work-Troops Scheduling Problem (WSP)**
asks in which order *work-troops* (WT), the teams that clear the roads, should
repair the blocked roads so that the affected population regains access as
quickly as possible.

The road network is an undirected graph `G = (V, E)`:

- **Origins** are the depots where the work-troops are based. They are also the
  points from which relief is sent.
- **Destinations** are the places that need relief. Each one has a population (its demand).
- Each edge `(i, j)` has a length `d`, a width `u` and a repair time `r`.
  An edge is blocked when `r > 0`. Its width `u` is the maximum number of
  work-troops that can work on it at the same time.

Time is split into `T = ⌈Σ r / Σ WT⌉` periods. In each period, every work-troop
works on the first blocked edge of the schedule that it can reach from its origin
through roads that are already open. It can work on that edge from either end, as
long as the width allows. An edge opens when its repair time reaches zero.
At the end of each period, the cost is the sum, over all destinations, of the
population times the shortest-path distance from the closest origin. A destination
that cannot be reached yet costs a large penalty. The goal is to find the
**order of the blocked edges** that minimizes the total cost over all periods.

The WSP is NP-hard. The paper studies a multi-period version of the problem from
Sakuraba et al. (2016) and proposes:

- a **ranking heuristic** (adapted from the LCH heuristic). It ranks each blocked
  edge by how many origin-to-destination shortest paths use it;
- a **local search** that swaps pairs of edges in the schedule (first improvement);
- a **GRASP**, with a randomized version of the ranking heuristic as its construction phase;
- an **ILS**, which starts from the ranking solution and perturbs it with random swaps.

The methods were tested on 264 simulated instances and on a real instance of
Port-au-Prince after the 2010 earthquake.

> **Note:** the Port-au-Prince (Haiti) instance is not distributed in this
> repository, because we do not have the rights to make it public. The Haiti
> results are reported in the papers below. Any instance in the same file format
> (see [`instances/README.md`](instances/README.md)) can be used with the code.

## Repository layout

```
.
├── src/            C/C++ source code
├── tests/          Unit tests (minunit)
├── instances/      Simulated instances (g1, g2, g3)
├── cplex/          .lp models, CPLEX scripts and logs used to compute optimal solutions
├── results/        Results tables and execution logs
├── scripts/        Helper scripts (TTT plots, id normalization)
└── Makefile
```

### `src/`

| File | Content |
|------|---------|
| `wsp.{h,c}` | Data structures (instance, solution) and instance loading |
| `wsp_math.{h,c}` | Solution evaluation: the period-by-period simulation of repairs and the objective function |
| `dijkstra.{h,cpp}` | Dijkstra's shortest paths from a super-source linked to all origins |
| `spanning_tree.{h,c}` | Union-find structure that checks whether a work-troop can reach an edge |
| `ranking.{h,c}` | Ranking heuristic |
| `localsearch.{h,c}` | Swap-based local search |
| `metaheuristics.{h,c}` | GRASP and ILS |
| `main.c`, `grasp.c`, `ils.c` | Programs that load an instance or run each method |

### `instances/`

- `vertices.txt` + `g1/`: the 88 simulated instances from Sakuraba et al. (2016).
- `g2/`, `g3/`: the same instances with every road width set to 1 and 2.

[`instances/README.md`](instances/README.md) describes the file formats.

### `cplex/`

- `g1/`, `g2/`, `g3/`: `.lp` models for each simulated instance, with CPLEX scripts (`run.bat`) and logs.
- `lp-generator/`: the C program (by C. Sakuraba) that generates the `.lp` models from the instance files.
- `multi-period/`: a variant of the generator and the models used to test different numbers of periods (`_q2`, `_q3`, …).
- `tests/`: models used for other tests.
- `replace_road_width.py`: builds the `g2`/`g3` instances and models by changing the road width.
- `logs/`: CPLEX logs for groups 2 and 3.

### `results/`

- `results-g1.csv`, `results-g2.csv`, `results-g3.csv`: upper and lower bounds from CPLEX and the results of LCH, GRASP and ILS for each group.

## Building and running

The code needs `g++` (C++11) and `make`.

```bash
make            # a.out: loads an instance and prints a summary
```

```bash
make grasp      # GRASP on a simulated instance
```

```bash
make ils        # ILS on a simulated instance
```

```bash
make tests      # unit tests (test_all.out)
```

```bash
make clean
```

Run the programs from the repository root, because the instance paths are relative to it:

```bash
./grasp
```

The instance and the parameters (GRASP `alpha`, ILS perturbation strength, number
of iterations) are set at the top of each `main` in `src/`.

## Citation

If you use this code or these instances, please cite:

```bibtex
@article{barbalho2023metaheuristics,
  author    = {Barbalho, Thiago Jobson and Santos, Andréa Cynthia and Aloise, Dario José},
  title     = {Metaheuristics for the work-troops scheduling problem},
  journal   = {International Transactions in Operational Research},
  year      = {2023},
  volume    = {30},
  number    = {2},
  pages     = {892--914},
  doi       = {10.1111/itor.12925},
  publisher = {Wiley}
}

@article{barbalho:hal-03355337,
  author    = {Barbalho, Thiago Jobson and Santos, Andréa Cynthia and Aloise, Dario José},
  title     = {Méthodes approchées pour le problème de planification du nettoyage de débris dans des réseaux urbains},
  journal   = {Cahiers de la logistique},
  publisher = {ISEL},
  series    = {Optimisation des opérations de déblaiement suite aux catastrophes en milieu urbain},
  volume    = {2},
  pages     = {1--11},
  year      = {2021},
  url       = {https://hal.science/hal-03355337},
  note      = {HAL: hal-03355337}
}

@inproceedings{barbalho:hal-03596190,
  author       = {Barbalho, Thiago Jobson and Santos, Andréa Cynthia and Aloise, Dario José},
  title        = {Metaheuristics for cleaning debris in urban networks after major disasters},
  booktitle    = {23ème congrès annuel de la Société Française de Recherche Opérationnelle et d'Aide à la Décision (ROADEF 2022)},
  address      = {Villeurbanne - Lyon, France},
  organization = {INSA Lyon},
  year         = {2022},
  month        = feb,
  url          = {https://hal.science/hal-03596190},
  note         = {HAL: hal-03596190}
}
```

## References

- C. S. Sakuraba, A. C. Santos, C. Prins, L. Bouillot, A. Durand, B. Allenbach.
  Road network emergency accessibility planning after a major earthquake.
  *EURO Journal on Computational Optimization*, 4(3–4):381–402, 2016.
- R. M. Aiex, M. G. C. Resende, C. C. Ribeiro. TTT plots: a Perl program to create
  time-to-target plots. *Optimization Letters*, 1:355–366, 2007. (`scripts/tttplots.pl`)

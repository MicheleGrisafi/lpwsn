# LPWSN repository

## Folder structure and content
- **app**: contains all of the source files for the various Neighbour Discovery (ND) primitives
	- nameSchema.txt: name schema used for identifying the various versions of the two primitives;
	- **general**: contains the final version of the implementation of the two primitives
		- *various files*
		- **simulations**: contains the simulation files required by cooja
	- **burst**: contains the various burst implementations (follows schema of **general**)
	- **scatter**: contains the various scatter implementations (follows schema of **general**)
- **scripts**: contains all of the scripts required for the processing of the simulation files.
	- **logs**: contains all of the generated logs, required for the optimiser to compute the performance.
	- simulate.sh: bash script that simulates, using cooja, a certain version of the ND protocols. A *fast* option, used also in the other scripts, allows the parsing of only a restricted set of simulations.
	- crossSimulation.sh: bash script that simulates all of the ND versions inside a certain folder
	- parser.py: python script that parses the logs of a single simulation and outputs its computed performance. Optimiser module.
	- crossParser.py: python script that parses all of the required logs, outputting the best version among the tested ones based on the various optimiser input parameters.
	- results.txt: file containing the specific performance metrics of the various parsed simulations.
	- graph.py: python script that generates the various graphs, based on the results.txt.
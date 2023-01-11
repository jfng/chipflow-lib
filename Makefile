.PHONY: init # Init local environemnt
init: 
	poetry install

.PHONY: lint # Lint code
lint: 
	poetry run pycodestyle --config=./.pycodestyle chipflow_lib
all:
	gcc -o main main.c
codeql:
	CCACHE_DISABLE=1 codeql database create codeqldb --language=cpp -vvv --overwrite
	codeql database analyze codeqldb/ --format csv --output results.csv test.qls -vvv

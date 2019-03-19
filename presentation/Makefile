svgs=$(wildcard figures/*.svg)

all: ${svgs:.svg=.pdf} main.pdf

main.pdf:
	pdflatex main.tex
	pdflatex main.tex

figures/%.pdf: figures/%.svg
	inkscape -D -z --file=$^ --export-pdf=$@

hard_spaces:
	! git status --porcelain | grep main.tex
	awk -i inplace -f hard_spaces.awk main.tex
	git diff --word-diff-regex=. main.tex

clean:
	rm -f main.pdf

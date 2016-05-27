all: CynaraReference.html

clean:
	rm -f CynaraReference.html complete.md

CynaraReference.html: style.css header.html footer.html complete.md
	pandoc -s -S -N --toc -c style.css -A footer.html -B header.html complete.md -f markdown -t html5 -o CynaraReference.html

complete.md: hardware.md
	cat hardware.md > complete.md

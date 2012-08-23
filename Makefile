VERSION = V1.0.5
package=gfold.$(VERSION)
packagename=gfold.$(VERSION).tar.gz 

all: program

debug: DataProcessor.hpp GFOLD.hpp Utility.hpp GeneInfo.hpp main.cc
	g++ -Wall -lgsl -lgslcblas -g main.cc -o gfold

program: DataProcessor.hpp GFOLD.hpp Utility.hpp GeneInfo.hpp main.cc
	g++ -O3 -Wall -lgsl -lgslcblas -g main.cc -o gfold

docu: doc/gfold.pod
	pod2man doc/gfold.pod > doc/gfold.man
	pod2html -css gfold.css --noindex --header --title="GFOLD $(VERSION)" doc/gfold.pod > doc/gfold_doc.html
	head -n 12 doc/gfold_doc.html > doc/gfold.html
	echo '<tr><td width="10%" class="block" align="left"> <img align=center src="gfold.png"></img> </td>'>> doc/gfold.html
	echo '<td class="block" valign="middle">' >> doc/gfold.html
	awk '{if (NR > 13) print}' doc/gfold_doc.html >> doc/gfold.html
	rm doc/gfold_doc.html


dist: DataProcessor.hpp GFOLD.hpp Utility.hpp GeneInfo.hpp main.cc doc/gfold.pod Makefile README
	mkdir ${package}
	cp -r DataProcessor.hpp GFOLD.hpp Utility.hpp GeneInfo.hpp main.cc doc Makefile README ${package}
	tar cvzf ${packagename} ${package}
	rm -rf ${package}
	cp ${packagename} gfold.V1.0.2.tar.gz doc/gfold.html doc/gfold.css doc/gfold.png web/
	tar cvzf gfoldweb.tar.gz web
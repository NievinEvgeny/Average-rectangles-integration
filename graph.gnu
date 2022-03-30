set terminal png
set output "graph.png"

set xrange [2:8]

set yrange [1:9]

set grid

set ytics 1

set xtics 2

set ylabel "Speedup"

set xlabel "Number of threads"

plot "./TestTable.txt" u 1:2 with linespoints lw 3 pt 7 ps 1 title "N (const) = 1e8", "./TestTable.txt" u 1:3 with linespoints lw 3 pt 7 ps 1 title "Linear Speedup"
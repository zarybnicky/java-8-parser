(TeX-add-style-hook
 "dokumentace"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("report" "12pt" "a4paper")))
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("inputenc" "utf8") ("geometry" "margin=1.8cm") ("babel" "czech" "slovak")))
   (add-to-list 'LaTeX-verbatim-environments-local "lstlisting")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "lstinline")
   (add-to-list 'LaTeX-verbatim-macros-with-delims-local "lstinline")
   (TeX-run-style-hooks
    "latex2e"
    "report"
    "rep12"
    "inputenc"
    "geometry"
    "amsmath"
    "babel"
    "amsfonts"
    "amssymb"
    "graphicx"
    "lmodern"
    "enumitem"
    "listings"))
 :latex)


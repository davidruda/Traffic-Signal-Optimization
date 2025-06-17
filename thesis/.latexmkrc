$aux_dir = 'tmp';
$pdf_mode = 4;

# If your TeX environment does not provide working LuaLaTeX, uncomment this:
#$pdf_mode = 1;

ensure_path('TEXINPUTS', 'tex//:');


# Add support for glossaries and glossaries-extra packages
# Copied from https://mirrors.ctan.org/support/latexmk/example_rcfiles/glossaries_latexmkrc
add_cus_dep( 'acn', 'acr', 0, 'makeglossaries' );
add_cus_dep( 'glo', 'gls', 0, 'makeglossaries' );
$clean_ext .= " acr acn alg glo gls glg";

sub makeglossaries {
    my ($base_name, $path) = fileparse( $_[0] );
    my @args = ( "-q", "-d", $path, $base_name );
    if ($silent) { unshift @args, "-q"; }
    return system "makeglossaries", "-d", $path, $base_name; 
}

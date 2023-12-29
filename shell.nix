{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    # nativeBuildInputs is usually what you want -- tools you need to run
    nativeBuildInputs = with pkgs.buildPackages; [ gnumake boost eigen ];
    buildInputs = with pkgs.buildPackages; [ python3 gnuplot zsh ];

    # Setting zsh as the dafault shell
    shellHook = ''
        export SHELL=${pkgs.zsh}/bin/zsh
    '';
}

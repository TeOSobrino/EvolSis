{
  description = "Genetic Algorithm for quadcopter Control";
  inputs = { 
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable"; 
  };

  outputs = { self, nixpkgs, ... }:
    let
        system = "x86_64-linux";
        
        pkgs = import nixpkgs {inherit system;
            #config.allowUnfree = true;
        };
    in {
      
        devShells.${system}.default = import ./shell.nix { inherit pkgs; };
        packages.${system} = { 
            default = with import nixpkgs { inherit system; };  stdenv.mkDerivation {
            name = "evolsys";
            src = self;
            
            nativeBuildInputs = with pkgs; [
                makeWrapper
                gnumake
                boost
            ];
            buildInputs = with pkgs; [
                python3                
            ];
            buildPhase = ''
                make all
            '';

            installPhase = ''
                mkdir -p $out
                mkdir -p $out/bin
                cp main $out/bin/evolsys
                wrapProgram $out/bin/evolsys \
      --prefix PATH : ${lib.makeBinPath [ python3 ]}
            ''; 
        };};
    };
}

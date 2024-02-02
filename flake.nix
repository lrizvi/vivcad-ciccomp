{
  description = "";
  
  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    poetry2nix = {
      url = "github:nix-community/poetry2nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, flake-utils, poetry2nix }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        inherit (poetry2nix.lib.mkPoetry2Nix { inherit pkgs; })
          mkPoetryEnv defaultPoetryOverrides;
        pypkgs-build-requirements = {
        };
      in {
        packages = {
          pyproject = mkPoetryEnv {
            projectDir = self;
            overrides = defaultPoetryOverrides.extend (self: super:
              builtins.mapAttrs (package: build-requirements:
                (builtins.getAttr package super).overridePythonAttrs (old: {
                  buildInputs = (old.buildInputs or [ ]) ++ (builtins.map (pkg: if builtins.isString pkg then builtins.getAttr pkg super else pkg) build-requirements);
                })
              ) pypkgs-build-requirements
            );
          };
          default = self.packages.${system}.pyproject;
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ self.packages.${system}.pyproject ];
          packages = with pkgs; [
            poetry
            mpfr
          ];
        };
      });
}

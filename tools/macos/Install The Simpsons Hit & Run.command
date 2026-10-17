#!/bin/bash
DIR="$(cd "$(dirname "$0")" && pwd)"
"$DIR/install_assets.sh" "$@"
read -p "Press [Enter] to exit..."

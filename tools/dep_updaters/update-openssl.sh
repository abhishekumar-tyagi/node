#!/bin/sh
set -e
# Shell script to update OpenSSL in the source tree to a specific version
# Based on https://github.com/nodejs/node/blob/main/doc/contributing/maintaining/maintaining-openssl.md

cleanup() {
  EXIT_CODE=$?
  [ -d "$WORKSPACE" ] && rm -rf "$WORKSPACE"
  exit $EXIT_CODE
}

download() {

  NEW_VERSION=$1

  if [ -z "$1" ]; then
    NEW_VERSION="$("$NODE" --input-type=module <<'EOF'
const res = await fetch('https://api.github.com/repos/quictls/openssl/releases');
if (!res.ok) throw new Error(`FetchError: ${res.status} ${res.statusText}`, { cause: res });
const releases = await res.json()
const latest = releases.find(({ tag_name }) => tag_name.contains('openssl-3.0'));
if(!latest) throw new Error(`Could not find latest release for 3.0`);
console.log(latest.tag_name.replace('openssl-', ''));
EOF
)"

  case "$NEW_VERSION" in
    *quic1) NEW_VERSION_NO_RELEASE_1="${NEW_VERSION%1}" ;;
    *) NEW_VERSION_NO_RELEASE_1="$NEW_VERSION" ;;
  esac
    VERSION_H="./deps/openssl/config/archs/linux-x86_64/asm/include/openssl/opensslv.h"
    CURRENT_VERSION=$(grep "OPENSSL_FULL_VERSION_STR" $VERSION_H | sed -n "s/^.*VERSION_STR \"\(.*\)\"/\1/p" | sed 's/+/-/g')
    echo "Comparing $NEW_VERSION_NO_RELEASE_1 with $CURRENT_VERSION"
    if [ "$NEW_VERSION_NO_RELEASE_1" != "$CURRENT_VERSION" ]; then
      echo "Skipped because openssl is on the latest version."
      exit 0
    fi
  fi

  echo "Making temporary workspace..."
  WORKSPACE=$(mktemp -d 2> /dev/null || mktemp -d -t 'tmp')

  cd "$WORKSPACE"

  echo "Fetching OpenSSL source archive..."
  OPENSSL_TARBALL="openssl-v$NEW_VERSION.tar.gz"
  curl -sL -o "$OPENSSL_TARBALL" "https://api.github.com/repos/quictls/openssl/tarball/openssl-$NEW_VERSION"
  log_and_verify_sha256sum "openssl" "$OPENSSL_TARBALL"
  gzip -dc "$OPENSSL_TARBALL" | tar xf -
  rm "$OPENSSL_TARBALL"
  mv quictls-openssl-* openssl

  echo "Replacing existing OpenSSL..."
  rm -rf "$DEPS_DIR/openssl/openssl"
  mv "$WORKSPACE/openssl" "$DEPS_DIR/openssl/"

  # Update the version number
  update_dependency_version "openssl" "$NEW_VERSION"

  echo "All done!"
  echo ""
  echo "Please git add openssl, and commit the new version:"
  echo ""
  echo "$ git add -A deps/openssl/openssl"
  echo "$ git add doc/contributing/maintaining/maintaining-dependencies.md"
  echo "$ git commit -m \"deps: upgrade openssl sources to quictls/openssl-$NEW_VERSION\""
  echo ""

  # The last line of the script should always print the new version,
  # as we need to add it to $GITHUB_ENV variable.
  echo "NEW_VERSION=$NEW_VERSION"
}

regenerate() {
  command -v perl >/dev/null 2>&1 || { echo >&2 "Error: 'Perl' required but not installed."; exit 1; }
  command -v nasm >/dev/null 2>&1 || { echo >&2 "Error: 'nasm' required but not installed."; exit 1; }
  command -v as >/dev/null 2>&1 || { echo >&2 "Error: 'GNU as' required but not installed."; exit 1; }
  perl -e "use Text::Template">/dev/null 2>&1 || { echo >&2 "Error: 'Text::Template' Perl module required but not installed."; exit 1; }

  echo "Regenerating platform-dependent files..."

  make -C "$DEPS_DIR/openssl/config" clean
  # Needed for compatibility with nasm on 32-bit Windows
  # See https://github.com/nodejs/node/blob/main/doc/contributing/maintaining/maintaining-openssl.md#2-execute-make-in-depsopensslconfig-directory
  sed -i 's/#ifdef/%ifdef/g' "$DEPS_DIR/openssl/openssl/crypto/perlasm/x86asm.pl"
  sed -i 's/#endif/%endif/g' "$DEPS_DIR/openssl/openssl/crypto/perlasm/x86asm.pl"
  make -C "$DEPS_DIR/openssl/config"

  echo "All done!"
  echo ""
  echo "Please commit the regenerated files:"
  echo ""
  echo "$ git add -A deps/openssl/config/archs deps/openssl/openssl"
  echo "$ git commit -m \"deps: update archs files for openssl\""
  echo ""
}

help() {
    echo "Shell script to update OpenSSL in the source tree to a specific version"
    echo "Sub-commands:"
    printf "%-23s %s\n" "help" "show help menu and commands"
    printf "%-23s %s\n" "download" "download and replace OpenSSL source code with new version"
    printf "%-23s %s\n" "regenerate" "regenerate platform-specific files"
    echo ""
    exit "${1:-0}"
}

main() {
  if [ ${#} -eq 0 ]; then
    help 0
  fi

  trap cleanup INT TERM EXIT

  BASE_DIR=$(cd "$(dirname "$0")/../.." && pwd)
  DEPS_DIR="$BASE_DIR/deps"

  [ -z "$NODE" ] && NODE="$BASE_DIR/out/Release/node"
  [ -x "$NODE" ] || NODE=$(command -v node)

    # shellcheck disable=SC1091
  . "$BASE_DIR/tools/dep_updaters/utils.sh"

  case ${1} in
    help | download | regenerate )
      $1 "${2}"
    ;;
    * )
      echo "unknown command: $1"
      help 1
      exit 1
    ;;
  esac
}

main "$@"

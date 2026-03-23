#!/usr/bin/env bash
set -a                # automatically export all variables
source /app/.env      # load environment variables from .env
set +a                # stop auto-exporting

# Run your executable
exec ./crud-exe

#!/bin/sh
(base64 script.ps1 | tr -d '\n'; echo) > script.ps1.base64

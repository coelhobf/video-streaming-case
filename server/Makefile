.PHONY: help up down restart logs status clean

help:
	@echo "MediaMTX Server Commands:"
	@echo "  up       - Start MediaMTX server"
	@echo "  down     - Stop MediaMTX server"
	@echo "  restart  - Restart MediaMTX server"
	@echo "  logs     - Show MediaMTX logs"
	@echo "  status   - Check server status"
	@echo "  clean    - Stop and remove containers"

up:
	docker compose up -d
	@sleep 2
	@echo "MediaMTX server started"
	@echo "Web UI: http://localhost:8080"
	@echo "SRT: srt://localhost:8890?streamid=publish:cam1"

down:
	docker compose down

restart: down up

logs:
	docker logs -f mediamtx

status:
	@docker ps --filter "name=mediamtx" --format "table {{.Names}}\t{{.Status}}" 2>/dev/null || echo "Not running"
	@curl -s http://localhost:9997/v3/config/global/get >/dev/null 2>&1 && echo "API: OK" || echo "API: Not responding"

clean: down
	docker compose down -v --remove-orphans
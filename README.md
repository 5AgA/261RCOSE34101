# CPU Scheduling Simulator

OS Term Project — C 언어로 구현한 CPU 스케줄링 시뮬레이터

## 구현 알고리즘

| 알고리즘 | 방식 |
|----------|------|
| FCFS | Non-Preemptive |
| SJF | Non-Preemptive |
| SJF | Preemptive (SRTF) |
| Priority | Non-Preemptive |
| Priority | Preemptive |
| Round Robin | Time Quantum = 2 |

## 빌드 및 실행

```bash
make        # 빌드
make run    # 빌드 + 실행
make clean  # 바이너리 삭제
```

> Linux / macOS 환경, gcc 필요

## 파일 구조

```
include/
  types.h          - Process, GanttEntry 구조체 및 상수 정의
  queue.h          - Ready / Waiting Queue
  process.h        - 프로세스 생성 및 출력
  scheduler.h      - 스케줄링 알고리즘 함수 선언
  gantt.h          - Gantt Chart 출력
  evaluation.h     - Waiting / Turnaround Time 계산
src/
  main.c           - 메뉴 및 전체 흐름
  process.c        - 랜덤 프로세스 생성 (PID, Arrival, Burst, Priority)
  queue.c          - 원형 큐 구현
  gantt.c          - 텍스트 기반 Gantt Chart
  evaluation.c     - 성능 계산 및 알고리즘 비교표
  fcfs.c           - FCFS 알고리즘
  sjf.c            - SJF (Non-Preemptive / Preemptive)
  priority_sched.c - Priority (Non-Preemptive / Preemptive)
  rr.c             - Round Robin
```

## 출력 예시

```
[Gantt Chart]
 ------- ------------- ------- -------
|  P2   |      P5      |  P1   |  P4   |
 ------- ------------- ------- -------
0       4             11      14      25

Average Waiting Time    : 6.80
Average Turnaround Time : 13.60
```

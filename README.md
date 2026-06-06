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

## 주요 기능

- **tick 단위 시뮬레이션** — 모든 알고리즘을 1틱씩 처리
- **I/O 지원** — CPU 실행 중 I/O 트리거, WAITING 상태 처리, 완료 후 READY 복귀
- **Gantt Chart** — CPU / I/O 각각 텍스트 기반 차트 출력
- **성능 평가** — 프로세스별 Completion / Turnaround / Waiting Time, 알고리즘 비교표
- **배치 테스트** — 랜덤 테스트케이스 최대 1000개 일괄 실행 + 알고리즘별 폴더 저장 + 통계 요약

## 빌드 및 실행

```bash
make        # 빌드
make run    # 빌드 + 실행
make clean  # 바이너리 삭제
```

> Linux / macOS 환경, gcc 필요

## 메뉴

```
 1. FCFS
 2. Non-Preemptive SJF
 3. Preemptive SJF
 4. Non-Preemptive Priority
 5. Preemptive Priority
 6. Round Robin
--------------------------------------
 7. Run All + Compare
 8. Show Process Table
 9. Generate New Processes
10. Batch Test
 0. Exit
```

- **1~6**: 개별 알고리즘 실행 → `results/[algo].txt` 저장
- **7**: 전체 알고리즘 실행 + 비교표 콘솔 출력 → `results/comparison.txt` 저장
- **9**: 새 랜덤 프로세스 생성
- **10**: 랜덤 개수(1~1000)의 테스트케이스 배치 실행 → `results/[algo]/case_XXXX.txt` + `results/batch_summary.txt`
- **0**: 결과 파일 전부 삭제 후 종료

## 파일 구조

```
include/
  config.h         - 상수 및 알고리즘 배열 선언 (ALGO_COUNT, ALGO_NAMES 등)
  types.h          - Process, GanttEntry 구조체 및 상수 정의
  runner.h         - run_one / run_all / run_batch 선언
  process.h        - 프로세스 생성 및 출력
  scheduler.h      - 스케줄링 알고리즘 함수 선언
  gantt.h          - Gantt Chart 출력
  evaluation.h     - Waiting / Turnaround Time 계산
  queue.h          - Ready Queue

src/
  main.c           - 메인 함수 + 메뉴 루프
  runner.c         - run_one / run_all / run_batch / 배치 통계 / 정합성 검사
  process.c        - 랜덤 프로세스 생성
  queue.c          - 원형 큐 구현
  gantt.c          - CPU / I/O Gantt Chart 출력
  evaluation.c     - 성능 계산 및 알고리즘 비교표
  fcfs.c           - FCFS 알고리즘
  sjf.c            - SJF (Non-Preemptive / Preemptive)
  priority_sched.c - Priority (Non-Preemptive / Preemptive)
  rr.c             - Round Robin

results/           - 실행 결과 저장 (종료 시 자동 삭제)
  fcfs.txt / sjf_np.txt / ...
  comparison.txt
  fcfs/ sjf_np/ ...   ← 배치 테스트 케이스별 폴더
  batch_summary.txt
  batch_errors.txt    ← 정합성 오류 발생 시에만 생성
```

## 출력 예시

**Gantt Chart**
```
[Gantt Chart]
 ------ ------------- ------ ------
|  P2  |      P5      |  P1  |  P4  |
 ------ ------------- ------ ------
0      4             11     14     25

[I/O Gantt Chart]
     0         10        20
     01234567890123456789012345
P3   _______________===__===__
P8   _______________________________=====
```

**Batch Summary**
```
========== Batch Test Summary (500 cases) ==========

[Performance Metrics]
Algorithm                   Avg WT (min/avg/max)          Avg TAT (min/avg/max)         Wins(WT/TAT)
-------------------------------------------------------------------------------------
FCFS                        4.00 / 21.00 / 46.00          10.20 / 28.46 / 55.20         15 / 15
Preemptive SJF              4.00 / 17.46 / 36.40          9.40 / 24.92 / 45.60          312 / 312

[Validation Results]
Total test cases  : 500
Error-free cases  : 500
Total violations  : 0  (all checks passed)
```
